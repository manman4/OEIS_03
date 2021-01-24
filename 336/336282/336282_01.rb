
def isHeapable(seq)
    sig = Array.new(seq.size, 0)
    sig[0] = 2
    seq[1..-1].each{|j|
        sig[j] = 2
        while j > -1
            j -= 1
            if sig[j] > 0
                sig[j] -= 1
                break
            end
        end
        if j == -1
            return false
        end
    }
    return true
end

def A336282(n)

    return 1 if n == 0
    cnt = 0
    (0..n - 1).to_a.permutation{|h|
      cnt += 1 if isHeapable(h)
    }
    cnt
end

(1..15).each{|i| p [i, A336282(i)]}

