M=30;
s=1;
t=3;
a(n) = my(A=1); for(i=0, n, A=1/(1-x/(1-x*A)^s)^t+x*O(x^n)); Vec(A); 
print1(a(M));

\\ G.f.: A(x) = B(x)^3 where B(x) is the g.f. of A300048.
a(n) = my(A=1); for(i=0, n, A=1/(1-x/(1-x*A)^s)^t+x*O(x^n)); Vec(A^(1/3));
print1(a(M));