M=12;

\\ Expansion of Product_{k>=1} 1/(1 - k^5 * x)^((1/2)^(k+1)).
a(n) = my(x='x+O('x^(n+1))); polcoef(prod(k=1, 700, 1/(1 - k^5 * x)^((1/2)^(k+1))), n) + 0.;
for(n=0, M, print1(round(a(n)),", "));
