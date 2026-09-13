\\ Verify proposed terms of A399825 independently of an OEIS b-file.
\\
\\ If M is the companion matrix of x^4-x-1, then
\\
\\   A050443(n) = trace(M^n),
\\
\\ because the traces begin 4,0,0,3 and satisfy
\\ a(n)=a(n-3)+a(n-4).  All matrix arithmetic below is performed
\\ modulo n, so even the largest listed candidate is checked quickly.

a050443_mod(n) =
{
  my(M = Mod([0,1,0,0;
              0,0,1,0;
              0,0,0,1;
              1,1,0,0], n));
  my(P = M^n);
  lift(sum(i=1, 4, P[i,i]));
};

verify(k) =
{
  my(n = 12*k + 3);
  my(r = a050443_mod(n));
  if(r != 0,
    error("verification failed: k=", k, ", n=", n, ", remainder=", r)
  );
  print(k, " OK: A050443(", n, ") mod ", n, " = ", r);
};

K = [0, 3006, 401781, 528531, 8105957031, 13635496761, 23200175781, 28778795256, 38384582031, 77202622656, 206457519531];

for(i=1, #K, verify(K[i]));

print("All ", #K, " proposed terms satisfy A399840(12*k+3) = 0.");
