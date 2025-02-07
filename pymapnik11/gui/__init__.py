
from .gui_frame import GuiBase
from .source_frame import SourceFileFrame
import wx

import pymapnik11 as mk
import sys, os.path



class MapObj:
    def __init__(self, parent, sourcefile):
        self.parent=parent
        self.sourcefile = sourcefile
        self.splits = None
        self.mps = None
        
        
        self.mml_contents = mk.open_yaml(self.sourcefile)
        
    def render(self, box, width=None, height=None, scale_factor=1, scale=None):
        if not self.mps:
            raise Exception("mapnikobj not created")
        
        return None
    
    def sourcefile_filenames(self):
        
        root,fn = os.path.split(self.sourcefile)
        res = [(fn, self.sourcefile)]
        
        for s in self.mml_contents['Stylesheet']:
            res.append((s, os.path.join(root, s)))
        return res
    

class GuiFrame(GuiBase):
    def __init__(self, parent=None):
        super().__init__(parent)
        
        self.Bind(wx.EVT_BUTTON, self.on_select_carto, self.select_carto_button)
        self.Bind(wx.EVT_TEXT_ENTER, self.on_carto_text_entry_return, self.carto_text_entry)
        self.Bind(wx.EVT_BUTTON, self.on_open_sourcefiles, self.open_sourcefiles_button)
        
        self.map_obj = None
        
        
        
    def on_select_carto(self, evt):
        
        with wx.FileDialog(self,
            "Open mml",
            style=wx.FD_DEFAULT_STYLE | wx.FD_FILE_MUST_EXIST,
            defaultDir = '/home/james/osm-carto-5.2',
            wildcard="osm carto mml (*.mml)|*.mml"      
            ) as fileDialog:

            if fileDialog.ShowModal() == wx.ID_CANCEL:
                return     # the user changed their mind

            # Proceed loading the file chosen by the user
            new_sourcefile = fileDialog.GetPath()
            self.update_carto(new_sourcefile)
                
            
    
    def on_carto_text_entry_return(self, evt):
        new_sourcefile = self.sourcefile_text_entry.GetText()
        self.update_carto(new_sourcefile)
        
    def update_carto(self, new_sourcefile):
        if self.map_obj and self.map_obj.sourcefile == new_sourcefile:
            return
        
        self.sourcefile_notebook.DeleteAllPages()
        self.map_obj = MapObj(self, new_sourcefile)
    
        self.carto_text_entry.SetValue(self.map_obj.sourcefile)
    
    def on_open_sourcefiles(self, evt):
        if not self.map_obj:
            return
        for name, path in self.map_obj.sourcefile_filenames():
            ff=SourceFileFrame(self.sourcefile_notebook, name, path)
            self.sourcefile_notebook.AddPage(ff, name)
        
        
        
           
    
        
    
        
        
        
        
        
        
        
def main():
    app=wx.App()
    frame=GuiFrame()
    if len(sys.argv)>1:
        frame.update_carto(sys.argv[1])
    
    #ff=SourceFileFrame(frame.sourcefile_notebook, 'demo.py', '/home/james/work/demo.py')
    #frame.sourcefile_notebook.AddPage(ff,'demo.py')
    
    frame.Show()
    
    app.MainLoop()
    
