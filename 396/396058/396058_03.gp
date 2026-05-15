M=80;

\\ q-二項係数 [n, r]_x
qbinom(n, r) = if(r < 0 || r > n, 0, prod(i=1, r, 1 - x^(n-i+1)) / prod(i=1, r, 1 - x^i));
 
\\ 交代和を含まない母関数 G_{k,m}(x)
G(k, m) = {
  my(M = min(k, m));
  x^(k*(k+1)/2) * sum(j=1, M,
    my(ex = (k-j+1)*(m-j));
    x^ex * qbinom(m-1, j-1) / prod(i=1, k-j, 1 - x^i);
  );
}
Vec(G(5, 3) + x*O(x^M))