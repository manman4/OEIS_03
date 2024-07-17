def A(n)
  h = {}
  (0..n).each{|i|
    (0..n).each{|j|
      k = i * i * i + j * j * j
      if h.has_key?(k)
        h[k] += 1
      else
        h[k] = 1
      end
    }
  }
  h.size
end

n = 100
b=[]
(0..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
  b << j
}
p b