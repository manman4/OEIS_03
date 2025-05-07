\\ Cf. A055870

pell(n) = ([2, 1; 1, 0]^n)[2, 1];

\\ n行目の係数を求める
\\ Let f(n, x) be defined as f(n, x) = Sum_{k=0..n} T(n,k) * x^k.
\\ f(n, x) = exp( -Sum_{k>=1} Pell(n*k)/Pell(k) * x^k/k ).
f(n) = my(x='x+O('x^(n+10))); exp( -sum(k=1, n+10, pell(n*k)/pell(k) * x^k/k ));
v(n) = Vec(f(n));
for(n=0, 10, print(v(n)));


\\ 0行目は1
a = [1];
for(n=1, 9, a = concat(a, v(n)[1..n+1]));
print(a);

\\ Sum_{k>=0} A099927(n+k,n) * x^k = 1/f(n+1, x).
for(n=0, 10, print(Vec(1/f(n+1))));
