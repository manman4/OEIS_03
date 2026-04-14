# A(n,k) = 1 if n = 0
# A(n,k) = k*(k+3) * A(n-1,k+6) - k*(k+2) * A(n-1,k+4) for n > 0
#
# We compute rows of A(n, k) for n = 0..max_n and k = 0..max_n,
# but to keep values correct we need a wider internal k range because
# the recurrence depends on k+4 and k+6.

def compute_rows(max_n)
  max_k = max_n + 6 * max_n
  prev = Array.new(max_k + 1, 1) # n = 0, A(0,k) = 1 for all k

  rows = Array.new(max_n + 1){Array.new(max_n + 1, 0)}
  rows[0] = (0..max_n).map { 1 }

  (1..max_n).each{|n|
    curr = Array.new(max_k + 1, 0)
    (0..max_k - 6).each{|k|
      curr[k] = k * (k + 3) * prev[k + 6] - k * (k + 2) * prev[k + 4]
    }
    rows[n] = curr[0..max_n]
    prev = curr
  }

  rows
end

max_n = 139
rows = compute_rows(max_n)

# Output read by rows: for n=0..max_n, for k=0..n, print A(k, n-k)
ary = []
(0..max_n).each{|n|
  (0..n).each{|k|
    ary << rows[k][n - k]
  }
}

(0..ary.size - 1).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}

