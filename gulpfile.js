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
            outputStyle: 'compressed'
        }).on('error', sass.logError))
        .pipe(sourcemaps.write())
        .pipe(gulp.dest('./public/css'));
});

gulp.task('js', function() {
    return gulp.src(['./resources/js/*.js'])
        .pipe(uglify({
            outputStyle: 'compressed'
        }))
        .pipe(concat('app.js'))
        .pipe(gulp.dest('./public/js'));
});

gulp.task('html', function() {
    return gulp.src(['./resources/views/*.htm', './resources/views/*.html'])
        .pipe(htmlmin({ collapseWhitespace: true }))
        .pipe(gulp.dest('./public'));
});

gulp.task('images', function() {
    return gulp.src(['./resources/images/**/*'])
        .pipe(gulp.dest('./public/images'));
});

gulp.task('clean-css', function() {
    return del('./public/css/*');
});

gulp.task('clean-js', function() {
    return del('./public/js/*');
});

gulp.task('clean-html', function() {
    return del('./public/*.htm*');
});

gulp.task('clean-images', function() {
    return del('./public/images/*');
});

gulp.task('refresh-sass', gulp.series('clean-css', 'sass'));
gulp.task('refresh-js', gulp.series('clean-js', 'js'));
gulp.task('refresh-html', gulp.series('clean-html', 'html'));
gulp.task('refresh-images', gulp.series('clean-images', 'images'));

gulp.task('watch', function () {
    gulp.watch('./resources/sass/**/*.scss', gulp.series('refresh-sass'));
    gulp.watch('./resources/js/**/*.js', gulp.series('refresh-js'));
    gulp.watch('./resources/views/**/*.htm*', gulp.series('refresh-html'));
    gulp.watch('./resources/images/**/*', gulp.parallel('refresh-images'));
});

gulp.task('clean-all', gulp.parallel('clean-css', 'clean-js', 'clean-html'));
gulp.task('build', gulp.parallel('sass', 'js', 'html'));
gulp.task('default', gulp.series('clean-all', 'build'));