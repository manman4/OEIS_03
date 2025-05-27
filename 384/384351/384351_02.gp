M=15;

\\ Expansion of Product_{k>=1} 1/(1 - k*(k+1)/2 * x)^((1/2)^(k+2)).
a(n) = my(x='x+O('x^(n+1))); polcoef(prod(k=1, 500, 1/(1 - k*(k+1)/2 * x)^((1/2)^(k+2))), n) + 0.;
for(n=0, M, print1(round(a(n)),", "));
