\\ 関数方程式
\\
\\   A(-log(1-c*x)/c)=(c+1)*A(x)-c*x
\\
\\ から直接 a(1),...,a(N) を求める。
\\ Stirling 数による漸化式は使わない。
a_from_general_definition(N,c)={
  my(X=x+O(x^(N+1)), P, M, b, v);
  if(c==0,error("c must be nonzero"));

  \\ A(x) に代入する級数
  P=-log(1-c*X)/c;

  \\ 方程式を
  \\
  \\   A(P)-(c+1)*A(X)=-c*X
  \\
  \\ として線形方程式を作る。
  M=matrix(N,N,n,k,
    polcoef(
      P^k/k!-(c+1)*X^k/k!,
      n
    )*n!
  );

  \\ 右辺 -c*X の E.g.f. 係数
  b=vector(N,n,
    if(n==1,-c,0)
  )~;

  \\ 未知係数 a(1),...,a(N) を解く。
  v=matsolve(M,b);
  Vec(v)
};
a_from_general_definition(20,2)
