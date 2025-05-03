my(N=30, x='x+O('x^N)); Vec(1/prod(k=0, 3, 1-(4*k+1)*x)^(1/4))

M=1000;
\\ Expansion of 1/( Product_{k=0..3} (1 - (4*k+1) * x) )^(1/4).
v(n) = my(x='x+O('x^(n+10))); 1/prod(k=0, 3, 1-(4*k+1)*x)^(1/4);
v = v(M);
for(n=0, M, if(denominator(polcoef(v, n)) != 1, print1(n, ", ")));

