
##Summary
We are going to create optimized implementations of image seam-carving on both NVIDIA GPU and multi-core x86 CPU platforms. We are also trying to come up with an algorithm which can switch between content aware (seam carving) and content agnostic (bilinear interpolation) retargeting after inspecting some global statistics of the image.

##Background  ( Keep the background unchanged )

##Challenges

1. The default use dynamic programming for the seam map computation, which is not GPU friendly and there is very low computation to communication ratio.

2. In order to speed up this algorithm, we definitely have to make approximations which would definitely result in degradation of the retargeted image quality. Now since there is no groundtruth for us, defining an evaluation metrics for us is as challenging as optimizing the baseline algorithm (if not more)

3. 





