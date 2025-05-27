M=15;

\\ Expansion of Product_{k>=1} 1/(1 - k*(k+1)*(k+2)*(k+3)/24 * x)^((1/2)^(k+4)).
a(n) = my(x='x+O('x^(n+1))); polcoef(prod(k=1, 500, 1/(1 - k*(k+1)*(k+2)*(k+3)/24 * x)^((1/2)^(k+4))), n) + 0.;
for(n=0, M, print1(round(a(n)),", "));
