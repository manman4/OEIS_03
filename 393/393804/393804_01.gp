\\ 393804_01.gp
\\ Expand directly in PARI/GP and take the maximal coefficient.
\\ WARNING: This is exponential; only very small n are feasible.

maxcoeff(n)=
{
  my(m = 2^n);
  my(keys = List());
  my(vals = List());
  my(e0 = vector(m, i, 0));
  listput(keys, e0);
  listput(vals, 1);

  for(k=0, n,
    my(new_keys = List());
    my(new_vals = List());
    for(idx=1, #keys,
      my(e = keys[idx]);
      my(c = vals[idx]);
      for(i=1, 2^k,
        my(e2 = vector(#e, j, e[j]));
        e2[i] += 1;
        my(found = 0);
        for(j=1, #new_keys,
          if(new_keys[j] == e2,
            new_vals[j] += c;
            found = 1;
            break;
          )
        );
        if(!found,
          listput(new_keys, e2);
          listput(new_vals, c);
        );
      );
    );
    keys = new_keys;
    vals = new_vals;
  );

  my(mx = 0);
  for(idx=1, #vals,
    if(vals[idx] > mx, mx = vals[idx]);
  );
  return(mx);
}

\\ Example run (change N as needed)
N = 5;
for(n=0, N, print(n, " ", maxcoeff(n)));
