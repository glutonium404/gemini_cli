first make sure to have gemini api key in you bashrc

```sh
GEMINI_API_KEY='<your api key>'
```

to convert to binary using node

```sh
# Install a fast bundler temporarily
npx esbuild index.js --bundle --platform=node --outfile=dist/bundle.js

# This downloads pkg temporarily and compiles your bundle
npx pkg dist/bundle.js --target node18-linux-x64 --output gemini-app
```
