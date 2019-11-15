'use strict';

var del = require('del');
var gulp = require('gulp');
var sass = require('gulp-sass');
var uglify = require('gulp-uglify');
var sourcemaps = require('gulp-sourcemaps');
var htmlmin = require('gulp-htmlmin');
var concat = require('gulp-concat');

sass.compiler = require('node-sass');

gulp.task('sass', function() {
    return gulp.src('./resources/sass/app.scss')
        .pipe(sass.sync({
            includePaths: ['./node_modules/bootstrap/scss/'],
            outputStyle: 'compressed'
        }).on('error', sass.logError))
        .pipe(sourcemaps.write())
        .pipe(gulp.dest('./public/css'));
});

gulp.task('js', function() {
    return gulp.src(['./resources/js/*.js', './node_modules/bootstrap/dist/js/bootstrap.js'])
        .pipe(uglify({
            outputStyle: 'compressed'
        }))
        .pipe(concat('app.js'))
        .pipe(gulp.dest('./public/js'));
});

gulp.task('html', function() {
    return gulp.src(['./resources/views/*.htm', './resources/views/*.html'])
        .pipe(htmlmin({ collapseWhitespace: true }))
        .pipe(gulp.dest('./public/views'));
});

gulp.task('watch', function () {
    gulp.watch('./resources/sass/**/*.scss', ['sass']);
    gulp.watch('./resources/sass/**/*.js', ['js']);
    gulp.watch('./resources/views/**/*.htm', ['htm']);
});

gulp.task('clean-css', function() {
    return del('./public/css/*');
});

gulp.task('clean-js', function() {
    return del('./public/js/*');
});

gulp.task('clean-views', function() {
    return del('./public/views/*');
});

gulp.task('clean-all', gulp.parallel('clean-css', 'clean-js', 'clean-views'));
gulp.task('build', gulp.parallel('sass', 'js', 'html'))
gulp.task('default', gulp.series('clean-all', 'build'));