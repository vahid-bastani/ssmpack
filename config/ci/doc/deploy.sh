#!/bin/bash
# modified from https://gist.github.com/domenic/ec8b0fc8ab45f39403dd

set -e # Exit with nonzero exit code if anything fails

git clone https://gitlab.com/vahid-bastani/ssmpack.git build/ssmpack
cp config/ci/doc/.gitlab-ci.yml .

# Get the deploy key by using Travis's stored variables to decrypt deploy_key.enc
cd config/ci/doc/
ENCRYPTED_KEY_VAR="encrypted_${ENCRYPTION_LABEL}_key"
ENCRYPTED_IV_VAR="encrypted_${ENCRYPTION_LABEL}_iv"
ENCRYPTED_KEY=${!ENCRYPTED_KEY_VAR}
ENCRYPTED_IV=${!ENCRYPTED_IV_VAR}
openssl aes-256-cbc -K $ENCRYPTED_KEY -iv $ENCRYPTED_IV -in deploy_key.enc -out deploy_key -d
chmod 600 deploy_key
eval `ssh-agent -s`
ssh-add deploy_key
cd ../../..

cd build/ssmpack

git add .
git commit -m "Deploy: ${SHA}"
git push