\\ ------------------------------------------------------------
\\ G.f.
\\   A(x) = 1 + x/(1-x)^t * A(x^s/(1-x)^s)
\\ の係数と E.g.f. を比較する。
\\
\\ v[n+1] は a(n) を表す。
\\ PARI のベクトルは添字が 1 から始まるためである。
\\ ------------------------------------------------------------


\\ 漸化式
\\   a(0) = 1,
\\   a(n) = Sum_{0 <= k <= (n-1)/s}
\\          binomial(n+t-2, s*k+t-1)*a(k)
\\ によって a(0),...,a(N) を求める。
a_by_recurrence(N,s,t)={
  my(v=vector(N+1));
  \\ v[1] = a(0)
  v[1]=1;
  for(n=1,N,
    v[n+1]=sum(k=0,(n-1)\s,
      binomial(n+t-2,s*k+t-1)*v[k+1]
    )
  );
  v
};


\\ G.f. の関数方程式をそのまま反復して
\\ A(x) を x^N の項まで求める。
\\
\\ 漸化式は使用していない。
A_by_definition_gf(N,s,t)={
  my(X=x+O(x^(N+1)), A=1+O(x^(N+1)));
  \\ 反復するたびに正しい係数が順に確定する。
  for(j=1,N+1,
    A=1+X*subst(A,x,X^s/(1-X)^s)/(1-X)^t
  );
  A
};




\\ 漸化式で求めた a(n) から E.g.f.
\\   E(x) = Sum_{n>=0} a(n)*x^n/n!
\\ を作る。
E_by_recurrence(N,s,t)={
  my(v=a_by_recurrence(N,s,t));
  sum(n=0,N,
    v[n+1]*x^n/n!
  )+O(x^(N+1))
};


\\ G.f. の定義から求めた A(x) の係数を取り出して、
\\ それぞれを n! で割り、E.g.f. に変換する。
\\
\\ ここでも漸化式は使用していない。
E_from_definition(N,s,t)={
  my(A=A_by_definition_gf(N,s,t));
  sum(n=0,N,
    polcoef(A,n)*x^n/n!
  )+O(x^(N+1))
};


\\ 次の E.g.f. の公式から直接級数を作る。
\\
\\ t >= 2 の場合：
\\   E(x) = 1 + D^(t-2) [
\\     exp(x)*Sum_{k>=0} a(k)*x^(s*k+t-1)/(s*k+t-1)!
\\   ]
\\
\\ t = 1 の場合：
\\   E(x) = 1 + Integral_0^x [
\\     exp(u)*Sum_{k>=0} a(k)*u^(s*k)/(s*k)!
\\   ] du
E_by_operator_formula(N,s,t)={
  my(v,M,X,F);
  if(t<1,error("t must be >= 1"));
  \\ この公式で使う a(k) を漸化式から求める。
  v=a_by_recurrence(N,s,t);
  \\ t-2 回微分すると精度が下がるため、
  \\ 最初に余分な次数まで計算しておく。
  M=N+max(t-2,0);
  X=x+O(x^(M+1));
  \\ 微分または積分を行う前の級数。
  F=exp(X)*sum(k=0,(N-1)\s,
    v[k+1]
    *X^(s*k+t-1)
    /(s*k+t-1)!
  );
  if(t==1,
    \\ t=1 のときは 1 回形式積分する。
    F=intformal(F,x),
    \\ t>=2 のときは t-2 回微分する。
    for(j=1,t-2,
      F=deriv(F,x)
    )
  );
  \\ 元の G.f. にある明示的な定数項 1 を加える。
  1+F+O(x^(N+1))
};


\\ 三通りに作った E.g.f. を比較する。
check_egf(N,s,t)={
  my(E1,E2,E3);
  \\ 漸化式から作った E.g.f.
  E1=E_by_recurrence(N,s,t);
  \\ G.f. の定義を反復して作った E.g.f.
  E2=E_from_definition(N,s,t);
  \\ 微分・積分公式から作った E.g.f.
  E3=E_by_operator_formula(N,s,t);
  print("recurrence = definition : ",E1==E2);
  print("recurrence = formula    : ",E1==E3);
  \\ 三つの級数も返す。
  [E1,E2,E3]
};


\\ check_egf(20,4,4);
\\ check_egf(20,3,3);
check_egf(20,2,3);
a_by_recurrence(20,2,3)