#! /bin/bash
#
# Copyright © 2013-2020 Graphia Technologies Ltd.
#
# This file is part of Graphia.
#
# Graphia is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Graphia is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Graphia.  If not, see <http://www.gnu.org/licenses/>.
#

CERTIFICATE_P12_FILE=certificate.p12

if [ -n "${APPLE_CERTIFICATE_P12_BASE64}" ]
then
  echo ${APPLE_CERTIFICATE_P12_BASE64} | base64 --decode > ${CERTIFICATE_P12_FILE}

  security create-keychain -p ${APPLE_KEYCHAIN_PASSWORD} build.keychain
  security default-keychain -s build.keychain
  security unlock-keychain -p ${APPLE_KEYCHAIN_PASSWORD} build.keychain
  security import ${CERTIFICATE_P12_FILE} -k build.keychain \
      -P ${APPLE_CERTIFICATE_PASSWORD} -T /usr/bin/codesign
  security set-key-partition-list -S apple-tool:,apple: -s -k ${APPLE_KEYCHAIN_PASSWORD} build.keychain

  rm -rf ${CERTIFICATE_P12_FILE}
  SIGNING_ENABLED=true
fi

BUILD_DIR=build

. ${BUILD_DIR}/variables.sh

# MacOS doesn't have readlink, hence this hackery
SCRIPT_DIR=$(cd "$(dirname "$0")"; pwd)

if [ -n "${SIGNING_ENABLED}" ]
then
  security unlock-keychain -p ${APPLE_KEYCHAIN_PASSWORD} build.keychain
fi

cd ${BUILD_DIR}

mkdir -p ${PRODUCT_NAME}.app/Contents/PlugIns/
cp -r plugins/*.dylib ${PRODUCT_NAME}.app/Contents/PlugIns/

cp CrashReporter ${PRODUCT_NAME}.app/Contents/MacOS/
cp MessageBox ${PRODUCT_NAME}.app/Contents/MacOS/
cp Updater ${PRODUCT_NAME}.app/Contents/MacOS/

mkdir -p ${PRODUCT_NAME}.app/Contents/Resources
cp -r ../source/app/examples \
  ${PRODUCT_NAME}.app/Contents/Resources

QML_DIRS=$(find ../source -name "*.qml" | xargs -n1 dirname | \
  sort | uniq | sed -e 's/\(^.*$\)/-qmldir=\1/')

# Any dylibs that we build ourselves will have an embedded RPATH that refers to
# the absolute Qt directory on the build machine, and the GateKeeper objects to
# this, so we manually remove them here
DYLIBS=$(find source -name "*.dylib")

for DYLIB in ${DYLIBS}
do
  RPATHS=$(otool -l ${DYLIB} | grep -A2 LC_RPATH | \
    grep "^ *path.*" | sed -e 's/^ *path \([^\(]*\).*$/\1/')

  for RPATH in ${RPATHS}
  do
    echo Removing RPATH ${RPATH} from ${DYLIB}...
    install_name_tool -delete_rpath ${RPATH} ${DYLIB}
  done
done

macdeployqt ${PRODUCT_NAME}.app \
  ${QML_DIRS} \
  -executable=${PRODUCT_NAME}.app/Contents/MacOS/${PRODUCT_NAME} \
  -executable=${PRODUCT_NAME}.app/Contents/MacOS/CrashReporter \
  -executable=${PRODUCT_NAME}.app/Contents/MacOS/MessageBox \
  -executable=${PRODUCT_NAME}.app/Contents/MacOS/Updater \
  -codesign="${APPLE_SIGN_ID}"

macdeployqt ${PRODUCT_NAME}.app ${MACDEPLOYQT_ARGS}

if [ -n "${SIGNING_ENABLED}" ]
then
  # Need to sign again because macdeployqt won't sign the additional executables
  echo "Resigning..."
  codesign --verbose --deep --force --options runtime --sign "${APPLE_SIGN_ID}" \
    ${PRODUCT_NAME}.app || exit $?
  echo "Verifying..."
  codesign --verbose --verify ${PRODUCT_NAME}.app || exit $?
  echo "OK"
fi

cat ${SCRIPT_DIR}/dmg.spec.json.template | sed \
  -e "s/_PRODUCT_NAME_/${PRODUCT_NAME}/g" \
  -e "s|_SCRIPT_DIR_|${SCRIPT_DIR}|g" > \
  dmg.spec.json
rm -f ${PRODUCT_NAME}-${VERSION}.dmg && appdmg dmg.spec.json \
  ${PRODUCT_NAME}-${VERSION}.dmg

if [ -z "${APPLE_NOTARIZATION_USERNAME}" ]
then
  echo "No notarization credentials supplied, skipping..."
  exit 0
fi

# Apple notarization
echo "Submitting notarization request..."
NOTARIZE_REQUEST=$(xcrun altool --notarize-app \
  --primary-bundle-id "app.Graphia.dmg" \
  --username "${APPLE_NOTARIZATION_USERNAME}" \
  --password "${APPLE_NOTARIZATION_PASSWORD}" \
  --asc-provider ${APPLE_NOTARIZATION_PROVIDER_SHORTNAME} \
  --file "${PRODUCT_NAME}-${VERSION}.dmg"
) || (echo "${NOTARIZE_REQUEST}" && exit 1)

echo "...success"

NOTARIZE_REQUEST_ID=$(echo "${NOTARIZE_REQUEST}" | grep "^RequestUUID" | \
  sed -e "s/^RequestUUID = //")

# Poll the notarization server for completion
echo "Waiting for notarization server response"
while sleep 5
do
  NOTARIZE_INFO=$(xcrun altool --notarization-info ${NOTARIZE_REQUEST_ID} \
      --username "${APPLE_NOTARIZATION_USERNAME}" \
      --password "${APPLE_NOTARIZATION_PASSWORD}")
  NOTARIZE_CHECK_RESULT=$?
  
  if [[ ${NOTARIZE_CHECK_RESULT} == 239 ]]
  then
    # 239 means the server doesn't recognise the UUID yet
    continue
  elif [[ ${NOTARIZE_CHECK_RESULT} != 0 ]]
  then
    echo "$? ${NOTARIZE_INFO}"
    exit ${NOTARIZE_CHECK_RESULT}
  fi

  NOTARIZE_STATUS=$(echo "${NOTARIZE_INFO}" | \
    grep "^\s*Status: " | awk '{$1 = ""; print $0}' | xargs)
  NOTARIZE_LOGFILE_URL=$(echo "${NOTARIZE_INFO}" | \
    grep "^\s*LogFileURL: " | awk '{$1 = ""; print $0}' | xargs)

  echo "...${NOTARIZE_STATUS}"

  if [[ ! "${NOTARIZE_STATUS}" =~ "in progress" ]] && [[ ! -z "${NOTARIZE_STATUS}" ]]
  then
    break
  fi

  sleep 55
done

echo "...complete"

curl -s --output notarization.log "${NOTARIZE_LOGFILE_URL}"

if [[ "${NOTARIZE_STATUS}" =~ "success" ]]
then
  echo "Stapling..."
  xcrun stapler staple "${PRODUCT_NAME}-${VERSION}.dmg" || exit $?
else
  echo "Notarization failed!"
  cat notarization.log
  exit 1
fi
