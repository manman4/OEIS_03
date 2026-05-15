M=80;

poch(n) = prod(i=1, n, 1 - x^i);

\\ q-二項係数 [n, r]_x
qbinom(n, r) = if(r < 0 || r > n, 0, prod(i=1, r, 1 - x^(n-i+1)) / prod(i=1, r, 1 - x^i));
 
\\ 交代和を含まない母関数 G_{k,m}(x)
G(k, m) = {
  my(M = min(k, m));
  sum(j=1, M,
    my(ex = (k-j+1)*m + j*(j-1)/2 + (k-j)*(k-j+1)/2);
    x^ex * qbinom(m-1, j-1) / poch(k-j)
  );
}
Vec(G(5, 3) + x*O(x^M))