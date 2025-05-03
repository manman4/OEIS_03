my(N=20, x='x+O('x^N)); Vec(1/prod(k=0, 10, 1-(11*k+1)*x)^(1/11))

M=1000;
\\ Expansion of 1/( Product_{k=0..10} (1 - (11*k+1) * x) )^(1/11).
v(n) = my(x='x+O('x^(n+10))); 1/prod(k=0, 10, 1-(11*k+1)*x)^(1/11);
v = v(M);
for(n=0, M, if(denominator(polcoef(v, n)) != 1, print1(n, ", ")));

