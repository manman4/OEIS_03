\\ G.f.: Sum_{j=1..8} q^(j^2+4) * q_binomial(7,j-1) / Product_{k=1..j} (1-q^k).
q_binomial(n, k) = if(k<0 || k>n, 0, prod(j=1, k, 1-q^(n-j+1))/prod(j=1, k, 1-q^j));
my(N=80, q='q+O('q^N)); concat([0, 0, 0, 0, 0], Vec(sum(j=1, 8, q^(j^2+4)*q_binomial(7, j-1)/prod(k=1, j, 1-q^k))))
