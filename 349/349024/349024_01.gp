M=20;
s=4;
t=3;
a(n) = local(A=1); for(i=0, n, A=1/(1-x/(1-x*A)^s)^t+x*O(x^n)); Vec(A); 
print1(a(M));