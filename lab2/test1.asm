.ORIG x3000;
AND R1, R1, #0;
ADD R1, R1, #-4;
LSHF R2, R1, #1;
RSHFL R3, R1, #1;
RSHFA R4, R1, #1;
HALT
.END
