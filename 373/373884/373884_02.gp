chi4(n)=if(n%2==0,0,if(n%4==1,1,-1));

\\ C(M) = sum(k=1,M, chi4(k))
Cs(M)=if(M<=0,0,if((M%4)==1 || (M%4)==2,1,0));

\\ G(M) = sum(k=1,M, chi4(k)*k^2)
G2(M)=
{
  my(q=M\4,r=M%4);
  if(M<=0,0,
    if(r==0,-8*q^2,
      if(r==1,8*q^2+8*q+1,
        if(r==2,8*q^2+8*q+1,-8*(q+1)^2)
      )
    )
  )
};

sumints(n)=n*(n+1)/2;
sumsq(n)=n*(n+1)*(2*n+1)/6;

\\ sum(d<=N, chi4(d)*floor(N/d))
S0chi(N)=
{
  my(s=0,k=1,t,u);
  while(k<=N,
    t=N\k; u=N\t;
    s += t*(Cs(u)-Cs(k-1));
    k=u+1
  );
  s
};

\\ sum(d<=N, d*floor(N/d)) = sum(m<=N, sigma(m))
S1(N)=
{
  my(s=0,k=1,t,u);
  while(k<=N,
    t=N\k; u=N\t;
    s += t*(sumints(u)-sumints(k-1));
    k=u+1
  );
  s
};

\\ sum(d<=N, d^2 * C(floor(N/d)))
S2chiA(N)=
{
  my(s=0,k=1,t,u);
  while(k<=N,
    t=N\k; u=N\t;
    s += Cs(t)*(sumsq(u)-sumsq(k-1));
    k=u+1
  );
  s
};

\\ sum(d<=N, chi4(d)*d^2*floor(N/d))
S2chiB(N)=
{
  my(s=0,k=1,t,u);
  while(k<=N,
    t=N\k; u=N\t;
    s += t*(G2(u)-G2(k-1));
    k=u+1
  );
  s
};

B2(N)=1 + 4*S0chi(N);
B4(N)=1 + 8*S1(N) - 32*S1(N\4);
B6(N)=1 + 16*S2chiA(N) - 4*S2chiB(N);

a2(m)=B2(10^m);
a4(m)=B4(10^m);
a6(m)=B6(10^m);

for(n=0, 10, print1(a2(n), ", "));
for(n=0, 10, print1(a4(n), ", "));
for(n=0, 10, print1(a6(n), ", "));

\\ for(n=0, 12, write("b373884.txt", n, " ", a(n)));


