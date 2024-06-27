def A(n)
  m = Math.sqrt(n).to_i
  cnt = 0
  (1..m).each{|i|
    (i + 1..m).each{|j|
      if i * i + n == 2 * j * j
        # p [i, j]
        cnt += 1
      end
    }
  }
  cnt
end

# p (1..119).map{|i| A(i)}
# p (1..119).map{|i| A(i * i)}
n = 10000
(1..n).each{|i|
  print i
  print ' '
  puts A(i)
}
