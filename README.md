# Homography and SFM using C++
Following was a simple test to understand the SFM pipeline. Started from understanding how to build essentials from OpenCV4.0 branch, then slowly ventured into detecting SIFT features and plotting them on the given images, to finding matches with another image on scene and then there were 2 paths:

## SIFT Feature Detection and Keypoint Drawing
<b>Scene Image:</b>  
![](compvisiontests/Output/homography/dst_1.jpg)

<b>Object 1 Image:</b>  
![](compvisiontests/Output/homography/src_1.jpg)

<b>Object 2 Image:</b>  
![](compvisiontests/Output/homography/src_2.jpg)

## Homography Calculation and Object Boudning Box Detection in Scene
<b>Object 1 in Scene</b>  
[0.5416514294202881, 0.0729575686538258, 22.24947839959597;  
 -0.1019638154153684, 0.5335849908567323, 99.63156264225833;  
 -4.907759606720315e-05, -5.599438906408732e-05, 1]   
  
<b>Homography Stats:</b>  
- Inliers: 376 
- Outliers: 79  
  
![](compvisiontests/Output/homography/src_1_dst_1.jpg)

<b>Object 2 in Scene</b>  
[0.2566283775759789, -0.1625235232701573, 482.7235216356567;  
 0.2120875833076415, 0.4323596462842703, 150.0643994463597;  
 -0.0001887560809912152, 0.0002476836832800081, 1] . 
  
<b>Homography Stats:</b>  
- Inliers: 174 
- Outliers: 54  
  
![](compvisiontests/Output/homography/src_2_dst_1.jpg)

## Finding camera Rotation and Translation for SFM

