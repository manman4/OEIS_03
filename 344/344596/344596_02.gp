M=51;

a(n) = if(n<2, n, 3*sumdiv(n, d, moebius(n/d)*(d-1)*d));
for(n=1, M, print1(a(n), ", "));