# T(n, k) = if(n*k==0, n^k, T(n-1, k)+n^4*T(n-1, k-1));
def A(n)
  ary = []
  prev = [1]
  ary << 1
  (1..n).each{|i|
    row = Array.new(i + 1, 0)
    row[0] = 1
    (1..i).each{|k|
      prev_k  = k     < prev.size ? prev[k]     : 0
      prev_k1 = k - 1 < prev.size ? prev[k - 1] : 0
      row[k] = prev_k + i**4 * prev_k1
    }
    ary.concat(row)
    prev = row
  }
  ary
end

n = 139
ary = A(n)
(0..ary.size - 1).each{|i|
  j = ary[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
