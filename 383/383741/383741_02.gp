\\ a(0) = 5; a(n) = Pell(5*n)/Pell(n) for n > 0.
pell(n) = ([2, 1; 1, 0]^n)[2, 1];
a(n) = if(n==0, 5, pell(5*n)/pell(n));
for(n=0, 16, print1(a(n),", "))