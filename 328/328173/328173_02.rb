def A(str)
  (0..str.size - 2).any?{|i| (str[0..i].to_i + str[i + 1..-1].to_i).to_s.split('').uniq.size == 1}
end

n = 10 ** 7
cnt = 0
(0..n).each{|i|
  if A((i * i).to_s)
    cnt += 1
    print cnt
    print ' '
    puts i
  end
}
