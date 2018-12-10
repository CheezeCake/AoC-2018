package main

import (
	"fmt"
	"strings"
)

func react(poly string) int {
	top := -1
	st := []byte(poly)

	for _, c := range poly {
		if top >= 0 && byte(c) != st[top] && strings.ToLower(string(c)) == strings.ToLower(string(st[top])) {
			top = top - 1
		} else {
			top = top + 1
			st[top] = byte(c)
		}
	}

	return top + 1
}

func reactAsync(ch chan int, input string, unit byte) {
	poly := strings.Replace(input, string(unit), "", -1)
	poly = strings.Replace(poly, strings.ToUpper(string(unit)), "", -1)
	ch <- react(poly)
}

func main() {
	var input string
	fmt.Scanln(&input)

	fmt.Println("part 1: ", react(input))

	ch := make(chan int, 26)

	for unit := 'a'; unit <= 'z'; unit = unit + 1 {
		go reactAsync(ch, input, byte(unit))
	}

	min := int(^uint(0) >> 1)
	for i := 0; i < 26; i = i + 1 {
		v := <-ch
		if v < min {
			min = v
		}
	}

	fmt.Println("part 2: ", min)
}
