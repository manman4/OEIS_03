# a(n) = a(n-1) + a(n-2) - a(n-5) - a(n-6) - a(n-7) + a(n-8) + a(n-9) + a(n-10) - a(n-13) - a(n-14) + a(n-15) for n > 33.
def a395257(n)
  ary = [0,0,0,1,0,1,2,3,4,6,7,10,13,18,22,29,34,43,51,63,
 74,90,104,124,143,167,191,222,251,288,325,369,413,
 467]
  (34..n).each{|i|
    ary << ary[i-1] + ary[i-2] - ary[i-5] - ary[i-6] - ary[i-7] + ary[i-8] + ary[i-9] + ary[i-10] - ary[i-13] - ary[i-14] + ary[i-15]
  }
  ary
end

# nまでの分割数
def a000041(n)
  ary = [1]
  (1..n).each{|i|
    s = 0
    (1..i).each{|j|
      k = j*(3*j-1)/2
      break if k > i
      s += (-1)**(j-1)*ary[i-k]
      k = j*(3*j+1)/2
      break if k > i
      s += (-1)**(j-1)*ary[i-k]
    }
    ary << s
  }
  ary
end

# a(n) = Sum_{k=0..n} A395257(k) * A000041(n-k).
def a(n)
  a395257 = a395257(n)
  a000041 = a000041(n)
  (0..n).map{|i|
    s = 0
    (0..i).each{|j|
      s += a395257[j] * a000041[i-j]
    }
    s
  }
end

n = 10000
seq = a(n)
(0..seq.size - 1).each{|i|
  j = seq[i]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
