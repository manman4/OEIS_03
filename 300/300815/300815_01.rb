def A(n)
  cnt = 0
  (1..n).each{|i|
    (i + 1..n).each{|j|
      if j % i > 0
        k = n - i - j
        if k > j
          if k % j > 0
            if k % i > 0
            #  p [n, [i, j, k]]
              cnt += 1
            end
          end
        end
      end
    }
  }
  cnt
end

n = 1000
(10..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
