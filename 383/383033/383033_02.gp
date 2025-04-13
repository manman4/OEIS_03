\\ Christian G. Bower氏の以下のコードを使用
\\ https://oeis.org/transforms_pari.txt
\\ ライセンスは上記に従う

\\ Cf. A136628
\\ read("transforms_pari.txt"); or
/* inverse weigh transform
 * this function ignores a_0 (A[1]) and assumes it to be 1
 * the output sequence always has b_0=0
 * trv_i_weigh_x is a version not making that assumption
 */

\\ 0から始まることに注意
{ trv_i_weigh(A) = local(i=-1,B=vector(#A-1,n,i=-i;i*1/n),C);
  A[1] = 1;
  C = log(Ser(A));
  A=vecextract(A,"2..");
  for(i=1, #A, A[i] = polcoeff(C,i));
  A = dirdiv(A,B);
  concat(0,A)
};

M=11;

\\ Product_{n>=1} (1 + x^n)^A(n,k) = (1 - (k-1)*x)/(1 - k*x).
f(n, k) = k^(n-1);
a(n, k) = my(A=vector(n+1, j, f(j-1, k))); trv_i_weigh(A)[n+1];
for(n=1, M, for(k=1, n, print1(a(k, n-k+1), ", "))); 