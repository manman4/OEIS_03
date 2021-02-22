seq = []
(1..25).each{|i|
  seq += [i] + [-1] * i
}

b = 2
while seq.include?(-1)
  i = seq.index(-1)
  seq[i] = 1
  k = 2
  blanks = b
  (i + 1..seq.size - 1).each{|s|
    if seq[s] == -1
      blanks -= 1
      if blanks < 0
        seq[s] = k
        blanks = k * b
        k += 1
      end
    end
  }
  b += 1
end
p seq

(1..10).each{|i|
  j = seq[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
