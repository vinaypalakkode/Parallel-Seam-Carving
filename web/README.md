
##Summary
We are going to create optimized implementations of image seam-carving on both NVIDIA GPU and multi-core x86 CPU platforms. We are also trying to come up with an algorithm which can switch between content aware (seam carving) and content agnostic (bilinear interpolation) retargeting after inspecting some global statistics of the image.

##Background  ( Keep the background unchanged )

##Challenges

1. The default seam carving algorithm uses dynamic programming for the seam map computation, which is not GPU friendly and there is very low computation to communication ratio.

2. In order to speed up this algorithm, we definitely have to make approximations which would  result in degradation of the retargeted image quality (aka. result in visual artifacts). Since there is no groundtruth for us to compare the modified algorithms ouputs to, defining an evaluation metrics for us is as challenging as optimizing the baseline algorithm itself(if not more)

3. Creating a test dataset  (input images) with different energy distribution /patterns so as to see the extent of artifacts introduced by the approximations.

##Resources( keep it unchanged)

##Goals and Deliverables
1. Implementation and analysis of the performance of the seam caving (like) algorithm on NVIDIA GTX 750 using the x86(Haswell) implementation as the baseline.
2. We will illustrate the test cases / scenarios where this algorithms performs well without sacrificing the quality of the output significantly. We will also report  the negative test cases.
3. As a nice to have feature, we will propose a hybrid algorithm which would choose either content aware or content agnostic retargeting to produce a "good" quality retargeted image.





