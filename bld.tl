-- vim: fdm=marker

return {

    {
        not_dependencies = {
            "lfs",
            "resvg",
            "rlwr",
        },
        artifact = "worms",
        main = "main.c",
        src = "src",
        flags = {
            "-fopenmp",
        },
        --[[
        debug_define = { },
        release_define = { },
        --]]
    },

}
