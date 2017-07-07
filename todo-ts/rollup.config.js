import fs from 'fs'
import svelte from 'rollup-plugin-svelte';
import nodeResolve from 'rollup-plugin-node-resolve';
import commonjs from 'rollup-plugin-commonjs';
import buble from 'rollup-plugin-buble';
import closure from 'rollup-plugin-closure-compiler-js';
import filesize from 'rollup-plugin-filesize';
import typescript from 'rollup-plugin-typescript';
import scss from 'rollup-plugin-scss';

const target = process.env.TARGET || ''
const srcDir = target || 'src'
const destFile = !target ? 'build.js' : (target + '.js')

var entryPath = srcDir + '/main.ts'
if (!fs.existsSync(entryPath))
    entryPath = srcDir + '/main.js'

// you can use UglifyJS instead of Closure — the result will be ~3.3kb
// import uglify from 'rollup-plugin-uglify';

const plugins = [ typescript({ typescript: require('typescript') }), nodeResolve(), commonjs(), scss(), svelte() ];
if ( process.env.production ) {
	plugins.push(
		buble(),
		closure({
			compilationLevel: 'SIMPLE'
		}),
		filesize()
	);
}

export default {
	entry: entryPath,
	dest: 'dist/' + destFile,
	format: 'iife',
	plugins,
	sourceMap: true
};
