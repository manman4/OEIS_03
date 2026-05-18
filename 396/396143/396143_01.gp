q_binomial(n, k) = if(k<0 || k>n, 0, prod(j=1, k, 1-q^(n-j+1))/prod(j=1, k, 1-q^j));

\\ G.f. of column k: Sum_{i=1..2*k} (-1)^(i-1) * q^(i*(i-1)/2+k) / Product_{j=1..2*k-i} (1-q^j) for k > 0. 
S(n,k) = if(k==0, 0^n, my(q='q+O('q^(n+1))); polcoef(sum(i=1, 2*k, (-1)^(i-1) * q^(i*(i-1)/2+k) / prod(j=1, 2*k-i, (1-q^j))), n));

\\ G.f. of column k: Sum_{i=1..2*k} q^(i^2+k) * q_binomial(2*k-1,i-1) / Product_{j=1..i} (1-q^j) for k > 0.
T(n,k) = if(k==0, 0^n, my(q='q+O('q^(n+1))); polcoef(sum(i=1, 2*k, q^(i^2+k) * q_binomial(2*k-1,i-1) / prod(j=1, i, (1-q^j))), n));

M=13;
for(n=0, M, for(k=0, n, print1(S(n, k),", ")));
for(n=0, M, for(k=0, n, print1(T(n, k),", ")));

E(n,k) = if(n==0 && k==0, 1, S(n,k) + T(n,k));
for(n=0, M, for(k=0, n, print1(E(n, k),", ")));
