const fs = require('fs')
const crypto = require('crypto')

// creates .h file out of index.html file
async function main() {
  let [preact, hooks, htm] = Array(3).fill('data:text/javascript;charset=utf-8,')
  await fetch('https://unpkg.com/htm@3.1.1/preact/standalone.mjs').then(r => r.text()).then(t => preact += t)
  let html = fs.readFileSync('index.html', 'utf8')
  html = html.replace('"https://unpkg.com/htm@3.1.1/preact/standalone.mjs"', () => JSON.stringify(preact))
  fs.writeFileSync('bundle.html', html)
  let cpp = '#pragma once\n#define HTML_BUNDLE_SIZE ' + html.length
  const hash = crypto.createHash('sha256').update(html).digest('hex').slice(0, 8)
  // mcvc requires strings to be 16kb or less, so chunk the string
  // cpp += `\nconst char html_bundle[] = R"${hash}(` + html + `)${hash}";`
  const chunks = []
  const chunkSize = 1024 * 16
  for (let i = 0; i < html.length; i += chunkSize) {
    chunks.push(html.slice(i, i + chunkSize))
  }
  cpp += `\nstatic std::string html_bundle = `
  for (let i = 0; i < chunks.length; i++) {
    cpp += `\nR"${hash}(` + chunks[i] + `)${hash}" `
  }
  cpp += ';'
  fs.writeFileSync('bundle.h', cpp)
}

main()

// Note: if bundle size gets too big (>65000 bytes), the html can be minified
// https://github.com/terser/html-minifier-terser
// can be used to minify the html (including html template literals)