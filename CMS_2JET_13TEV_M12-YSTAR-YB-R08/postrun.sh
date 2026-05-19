#!/bin/bash

mkdir tmp

name="CMS_2JET_13TEV_3D"
extension=".pineappl.lz4"

# Grids on ploughshare were labelled incorrectly. The mapping is as below:
names[5]="_yb_BIN1_ys_BIN1"
names[10]="_yb_BIN1_ys_BIN2"
names[14]="_yb_BIN1_ys_BIN3"
names[17]="_yb_BIN1_ys_BIN4"
names[19]="_yb_BIN1_ys_BIN5"

names[6]="_yb_BIN2_ys_BIN1"
names[11]="_yb_BIN2_ys_BIN2"
names[15]="_yb_BIN2_ys_BIN3"
names[18]="_yb_BIN2_ys_BIN4"

names[7]="_yb_BIN3_ys_BIN1"
names[12]="_yb_BIN3_ys_BIN2"
names[16]="_yb_BIN3_ys_BIN3"

names[8]="_yb_BIN4_ys_BIN1"
names[13]="_yb_BIN4_ys_BIN2"

names[9]="_yb_BIN5_ys_BIN1"

# Kinematics information for each grid
kin_info["_yb_BIN1_ys_BIN1"]="yb<0.5 ys<0.5"             
kin_info["_yb_BIN1_ys_BIN2"]="yb<0.5 0.5<=ys<1.0"        
kin_info["_yb_BIN1_ys_BIN3"]="yb<0.5 1.0<=ys<1.5"        
kin_info["_yb_BIN1_ys_BIN4"]="yb<0.5 1.5<=ys<2.0"        
kin_info["_yb_BIN1_ys_BIN5"]="yb<0.5 2.0<=ys<2.5"        

kin_info["_yb_BIN2_ys_BIN1"]="0.5<=yb<1.0 ys<0.5"        
kin_info["_yb_BIN2_ys_BIN2"]="0.5<=yb<1.0 0.5<=ys<1.0"   
kin_info["_yb_BIN2_ys_BIN3"]="0.5<=yb<1.0 1.0<=ys<1.5"   
kin_info["_yb_BIN2_ys_BIN4"]="0.5<=yb<1.0 1.5<=ys<2.0"   

kin_info["_yb_BIN3_ys_BIN1"]="1.0<=yb<1.5 ys<0.5"        
kin_info["_yb_BIN3_ys_BIN2"]="1.0<=yb<1.5 0.5<=ys<1.0"   
kin_info["_yb_BIN3_ys_BIN3"]="1.0<=yb<1.5 1.0<=ys<1.5"   

kin_info["_yb_BIN4_ys_BIN1"]="1.5<=yb<2.0 ys<0.5"        
kin_info["_yb_BIN4_ys_BIN2"]="1.5<=yb<2.0 0.5<=ys<1.0"   

kin_info["_yb_BIN5_ys_BIN1"]="2.0<=yb<2.5 ys<0.5"        


# Convert the grids to pineappl format
for i in {5..19}; do
    bin=${names[${i}]}
    dst="${name}${bin}${extension}"
    printf -v src 'grid_0%02d.tab.gz' "$i"
    pineappl import "$src" "tmp/$dst" NNPDF40_nnlo_as_01180
    current_bin=${kin_info[${bin}]}
    pineappl write --set-key-value kin_bin "${current_bin}" "tmp/$dst" "$dst"
done

rm -r tmp
rm *.tab.gz
rm *.appl