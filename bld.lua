

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




   },

}
