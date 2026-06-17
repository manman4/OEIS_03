\\ E.g.f. A(x) satisfies A'(x) = 1/(1 - A(x)*A'(x))^k, with A(0)=0.
A_ser(k, N)=
{
  my(x = 'x + O('x^(N+1)));
  my(A = x + O('x^(N+1)));
  for(i=1, N,
    A = intformal(1/(1 - A*deriv(A))^k) + O('x^(N+1))
  );
  A
};

Vec(serlaplace(A_ser(0, 10)))
Vec(serlaplace(A_ser(1, 10)))
Vec(serlaplace(A_ser(2, 10)))
Vec(serlaplace(A_ser(3, 10)))
Vec(serlaplace(A_ser(4, 10)))

print("Now using series reversion...");

\\ Let R(x) = Series_Reversion( (1 - (1-2*(k+1)*x) * (1-x)^(2*k))/(2*(2*k+1)) ).
\\ A(x) = R(x) * (1-R(x))^k.
Aser(k, N)=
{
  my(z = 'x + O('x^N));
  my(R = serreverse((1 - (1 - 2*(k+1)*z)*(1-z)^(2*k)) / (2*(2*k+1))));
  R * (1-R)^k;
};

Vec(serlaplace(Aser(0, 20) - A_ser(0, 10)))
Vec(serlaplace(Aser(1, 20) - A_ser(1, 10)))
Vec(serlaplace(Aser(2, 20) - A_ser(2, 10)))
Vec(serlaplace(Aser(3, 20) - A_ser(3, 10)))
Vec(serlaplace(Aser(4, 20) - A_ser(4, 10)))

    