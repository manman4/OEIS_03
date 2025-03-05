M=28;

J(n, k) = sumdiv(n, d, d^k*moebius(n/d));
a(n) = J(n, 9)/J(n, 3);
for(n=1, M, print1(a(n), ", "))



