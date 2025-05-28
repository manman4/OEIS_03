M=12;

\\ Expansion of Product_{k>=1} 1/(1 - k^2 * x)^((1/30) * (2/3)^k).
a(n) = my(x='x+O('x^(n+1))); polcoef(prod(k=1, 700, 1/(1 - k^2 * x)^((1/30) * (2/3)^k)), n) + 0.;
for(n=0, M, print1(round(a(n)),", "));
