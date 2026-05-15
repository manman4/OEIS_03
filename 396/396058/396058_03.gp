M=80;

\\ q-二項係数 [n, r]_q
q_binomial(n, r) = if(r < 0 || r > n, 0, prod(i=1, r, 1 - q^(n-i+1)) / prod(i=1, r, 1 - q^i));
 
\\ 交代和を含まない母関数 G_{k,m}(q)
G(k, m) = {
  my(M = min(k, m));
  q^(k*(k+1)/2) * sum(j=1, M,
    my(ex = (k-j+1)*(m-j));
    q^ex * q_binomial(m-1, j-1) / prod(i=1, k-j, 1 - q^i);
  );
}
Vec(G(5, 3) + q*O(q^M))