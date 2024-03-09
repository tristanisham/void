class Base64 {
    foreign static encode(data)

    foreign static decode(data)
}

class Base32 {
    foreign static encode(data)

    foreign static decode(data)
}

class Base16 {
    foreign static encode(data)

    foreign static decode(data)
}

class Markdown {
    foreign static toHTML(buff)
}

foreign class JSON {
    construct encode(map) {}

    static mapKeys(map) {
        if (map is Map) {
            return map.keys
        }
    }    
}