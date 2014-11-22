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

if ~exists('PLOT') then
    PLOT = 1;
end

if ~exists('HIGH_QUALITY') then
    HIGH_QUALITY = 1;
end

f_input = INPUT_FREQ;
f_output = OUTPUT_FREQ;
//DEST_FILE_STAGE_1='stage1.txt'
//DEST_FILE_STAGE_2='stage2.txt'

f_mid_stage = f_input * 2;
gcd_in_out = gcd(int32([f_mid_stage, f_output]));
M2 = double(f_output / gcd_in_out);
L2 = double(f_mid_stage / gcd_in_out);

have_stage_1 = 1;
have_stage_2 = (M2 > 1)

//
// Stage 1 - x2 Oversampling filter
//
forder_1 = 1024;
alpha_1 = 21.0;

if (f_output >= f_input) then
    f_lim1 = (0.5) / 2.;
else
    f_lim1 = (0.5 * f_output / f_input) / 2.;
end
fc1 = f_lim1 - (((alpha_1 ** 0.91) * 0.45) / forder_1);

[wft1, wfm1, fr1] = wfir('lp', forder_1, [fc1 0], 'kr', [alpha_1 0]);


// 
// Stage 2 - Poly-phase filter
//

if (M2 > 1) then
    // fc2_base = f_lim1;   // <- specialized for specified input/output frequency
    fc2_base = 0.25;        // <- can be shared among several combinations of input/output frequency (no L2 dependency)

    if (HIGH_QUALITY == 1) then
        forder_2_k = 16;
        alpha_2 = 16.8;
        fc_shift = 0.16;
    else
        forder_2_k = 8;
        alpha_2 = 8.6;
        fc_shift = 0.14;
    end
    forder_2 = forder_2_k * M2;
    f_lim2 = ((1.0 - fc2_base) / M2);
    fc2 = (fc2_base + fc_shift) / M2;
    
    [wft2, wfm2, fr2] = wfir('lp', forder_2, [fc2 0], 'kr', [alpha_2 0]);
else
    forder_2_k = 1;
    forder_2 = 1;
    alpha_2 = 0;
    
    wft2 = [1];
end

// save
if exists('DEST_FILE_STAGE_1') then
    comment=sprintf("// << Parameters (STAGE %d)>>\n// INPUT_FREQ = %d\n// OUTPUT_FREQ = %d\n// FORFER      = %d\n// ALPHA       = %f\n", 1, f_input, f_output, forder_1, alpha_1);
    mod_wft1 = wft1';
    fprintfMat(DEST_FILE_STAGE_1, mod_wft1, '%.8ef,', comment);
end

if exists('DEST_FILE_STAGE_2') then
    comment=sprintf("// << Parameters (STAGE %d)>>\n// INPUT_FREQ = %d\n// OUTPUT_FREQ = %d\n// FORFER      = (%d x %d)\n// ALPHA       = %f\n// HIGH QUALITY= %d\n", 2, f_input, f_output, forder_2_k, M2, alpha_2, HIGH_QUALITY);

    // convert to poly-phase optimized form
    mod_wft2 = wft2 * M2;
    mod_wft2 = resize_matrix(mod_wft2, 1, forder_2);
    mod_wft2 = matrix(mod_wft2, M2, -1);

    fprintfMat(DEST_FILE_STAGE_2, mod_wft2, '%.8ef,', comment);
end


//
// plot
//
if (PLOT == 1) then
    clf();
end

if (PLOT == 1) & (have_stage_1) then
    //
    // Stage - 1
    //

    // re-calculate frequency response
    [wfm1, fr1] = frmag(wft1, 4096);

    if have_stage_2 then
        subplot(2,1,1);
    end

    plot(fr1, 20*log10(wfm1), 'red');
    plot([fc1 fc1], [20 -240], 'green:')
    plot([f_lim1 f_lim1], [20 -240], 'blue:')
    plot([0.5 0.5], [20 -240], 'cyan:')

    // draw grid
    xgrid;

    // set axis style
    a = gca();
    a.ticks_format = "%.2f";
    a.tight_limits = 'on';
    a.data_bounds=[0, -200; (0.5)*1.05, 20];

    // labels
    xlabel(sprintf("[STEGE1] input: %d - output: %d - cutoff: %d - stop: %d", int(f_input), int(f_output), int(fc1 * f_mid_stage), int(f_lim1 * f_mid_stage)));
    ylabel("dB")

    // legends
    legend(['STAGE 1 filter', 'cut off', 'stop', 'input x2 nyquist']);
end

if (PLOT == 1) & (have_stage_2) then
    //
    // Stage - 2
    //

    // re-calculate frequency response
    [wfm2, fr2] = frmag(wft2, 16384);
    
    // calculate aliasing of filter 1
    [wfm1_s, fr1_s] = frmag(wft1, 1024);
    fr1_alias = []
    wfm1_alias = []
    for i = 1:3;
        fr1_alias = cat(2, fr1_alias, fr1_s)
        wfm1_alias = cat(2, wfm1_alias, wfm1_s)

        fr1_s = fr1_s + 0.5;    // freqeuency shift
        wfm1_s = wfm1_s($:-1:1);  // inverse horizontal
    end

    if have_stage_1 then
        subplot(2,1,2);
    end

    // draw grid
    xgrid;

    plot(fr1_alias/M2, 20*log10(wfm1_alias), 'black');
    plot(fr2, 20*log10(wfm2), 'magenta');
    plot([fc2 fc2], [20 -240], 'green:')
    plot([f_lim2 f_lim2], [20 -240], 'blue:')
    plot([0.5/M2 0.5/M2], [20 -240], 'cyan:')

    // draw grid
    xgrid;

    // set axis style
    a = gca();
    
    if (M2 < 5) then
        a.ticks_format = "%.2f";
    elseif (M2 < 50) then
        a.ticks_format = "%.3f";
    else
        a.ticks_format = "%.4f";
    end
    a.tight_limits = 'on';
    a.data_bounds=[0, -200; (0.5 * 2 / M2)*1.05, 20];

    // labels   
    xlabel(sprintf("[STAGE2] M: %d - L: %d - cutoff: %d - stop: %d", int(M2), int(L2), int(fc2 * M2 * f_mid_stage), int(f_lim2 * M2 * f_mid_stage)));
    ylabel("dB")

    // legends
    if have_stage_1 then
        legend(['STAGE 1 filter', 'STAGE 2 filter', 'cut off', 'stop', 'output nyquist']);
    else
        legend(['Input signal', 'STAGE 2 filter', 'cut off', 'stop', 'output nyquist']);
    end
end
