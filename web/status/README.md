##Status Update

1. We have implemented a seam-carving (like) algorithm on GTX 750 which is about 8x faster than a baseline CPU implementation of the same for average cases.

2. We are still working on finding an optimal criterion for switching between seamcarving and scaling

3. We will shortly update the following graphs
 

    3.1 GPU-Speed up vs no of seams removed (width reduced) for a given resolution ( average figures)
    
    3.2 GPU-Speed up vs the (width ) size of the image for a given number of seams to be removed.
    
    3.3 A surface plot combining above two with more data points for completeness
    

##Schedule
April 03 - April 09: Sequential C++ implementation of the Seam Carving algorithm. Profile the energy computation and seam map calculation stages and also come up with a test harness. Profile this naive algorithm on x86.  
April 10 - April 16: Figure out the cases where seamcarving performs poorly than scaling in terms of quality and vice verca.  
April 17 - April 23: Finish and profile energy map computation and start implementing seam map computation on CUDA.  
April 24 - April 30: Complete seam map computation and profile it.  
May 01 - May 07: Final iterations of bug-fixing, optimization, performance analysis and final report preparation.  
May 08 - May 11: Prepare the presentation and hand in the code.  







