\\ a(0) = 4; a(n) = Pell(4*n)/Pell(n) for n > 0.
pell(n) = ([2, 1; 1, 0]^n)[2, 1];
a(n) = if(n==0, 4, pell(4*n)/pell(n));
for(n=0, 19, print1(a(n),", "))