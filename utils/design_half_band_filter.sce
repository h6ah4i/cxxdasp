//
//    Copyright (C) 2014 Haruki Hasegawa
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
//

// This file is a Scilab script file

// clear all variables (except for protected variables)
clear();

//
// parameters
//
if ~exists('INPUT_FREQ') then
    INPUT_FREQ = 44100;
end

if ~exists('OUTPUT_FREQ') then
    OUTPUT_FREQ = 48000;
end

if ~exists('FORDER') then
    FORDER = (128 + 1);
end

if ~exists('ALPHA') then
    ALPHA = 14.0;
end

if ~exists('PLOT') then
    PLOT = 1;
end

fc = 0.25;
forder = FORDER;
alpha = ALPHA;

[wft, wfm, fr] = wfir('lp', forder, [fc 0], 'kr', [alpha 0]);

// save
if exists('ORIG_DEST_FILE') then
    comment=sprintf("// << Half band filter (Original coefficients) >>\n// FORDER = %d\n// ALPHA       = %f\n", forder, alpha);

    fprintfMat(ORIG_DEST_FILE, wft', '%.8ef,', comment);
end

if exists('MOD_DEST_FILE') then
    comment=sprintf("// << Half band filter (Modified coefficients) >>\n// FORDER = %d\n// ALPHA       = %f\n", forder, alpha);

    // extract half
    t = wft(:,1:int(length(wft)/2));

    // extract even values (= non zero)
    t = matrix(t, 2, int(length(t)/2));
    t = t(2,:);
    
    // normalize
    t = t * (0.5 / sum(t));

    // transpose
    t = t';

    fprintfMat(MOD_DEST_FILE, t, '%.8ef,', comment);
end

//
// plot
//
if (PLOT == 1) then
    clf();

    // re-calculate frequency response
    [wfm, fr] = frmag(wft, 4096);

    plot(fr, 20*log10(wfm), 'red');
    plot([fc fc], [20 -240], 'green:')

    // draw grid
    xgrid;

    // set axis style
    a = gca();
    a.ticks_format = "%.2f";
    a.tight_limits = 'on';
    a.data_bounds=[0, -200; (0.5)*1.05, 20];

    // labels
    xlabel(sprintf("Halfband filter (FORDER: %d)", int(forder)));
    ylabel("dB")

    // legends
    legend(['filter', '1/2 nyquist']);
end
